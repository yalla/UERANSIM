//
// This file is a part of UERANSIM open source project.
// Copyright (c) 2021 ALİ GÜNGÖR.
//
// The software and all associated files are licensed under GPL-3.0
// and subject to the terms and conditions defined in LICENSE file.
//

#pragma once

#include <array>
#include <bitset>
#include <lib/nas/nas.hpp>
#include <lib/nas/timer.hpp>
#include <ue/nts.hpp>
#include <ue/types.hpp>
#include <utils/nts.hpp>

namespace nr::ue
{

class NasMm;

class NasSm
{
  private:
    TaskBase *m_base;
    UeTimers *m_timers;
    std::unique_ptr<Logger> m_logger;
    NasMm *m_mm;

    std::array<PduSession *, 16> m_pduSessions{};
    std::array<ProcedureTransaction, 255> m_procedureTransactions{};

    friend class UeCmdHandler;

  public:
    NasSm(TaskBase *base, UeTimers *timers);

  public:
    /* Base */
    void onStart(NasMm *mm);
    void onQuit();
    void establishInitialSessions();

    /* Transport */
    void receiveSmMessage(const nas::SmMessage &msg);

    /* Resource */
    void localReleaseSession(int psi);
    void localReleaseAllSessions();
    bool anyEmergencySession();
    void handleUplinkStatusChange(int psi, bool isPending);
    bool anyUplinkDataPending();
    bool anyEmergencyUplinkDataPending();
    std::bitset<16> getUplinkDataStatus();
    std::bitset<16> getPduSessionStatus();

    /* Session Release */
    void sendReleaseRequest(int psi);
    void sendReleaseRequestForAll();

  private:
    /* Transport */
    void sendSmMessage(int psi, const nas::SmMessage &msg);
    void receiveSmStatus(const nas::FiveGSmStatus &msg);
    void sendSmCause(const nas::ESmCause &cause, int pti, int psi);

    /* Allocation */
    int allocatePduSessionId(const SessionConfig &config);
    int allocateProcedureTransactionId();
    void freeProcedureTransactionId(int pti);
    void freePduSessionId(int psi);

    /* Session Establishment */
    void sendEstablishmentRequest(const SessionConfig &config);
    void receiveEstablishmentAccept(const nas::PduSessionEstablishmentAccept &msg);
    void receiveEstablishmentReject(const nas::PduSessionEstablishmentReject &msg);
    void receiveEstablishmentRoutingFailure(const nas::PduSessionEstablishmentRequest &msg);

    /* Session Release */
    void receiveReleaseReject(const nas::PduSessionReleaseReject &msg);
    void receiveReleaseCommand(const nas::PduSessionReleaseCommand &msg);

    /* Timer */
    std::unique_ptr<nas::NasTimer> newTransactionTimer(int code);
    void onTimerExpire(nas::NasTimer &timer);
    void onTransactionTimerExpire(int pti);

    /* Procedure */
    bool checkPtiAndPsi(const nas::SmMessage &msg);
    void abortProcedureByPti(int pti);
    void abortProcedureByPtiOrPsi(int pti, int psi);

  public:
    /* Interface */
    void handleNasEvent(const NwUeNasToNas &msg); // used by NAS
    void onTimerTick();                           // used by NAS
};

} // namespace nr::ue